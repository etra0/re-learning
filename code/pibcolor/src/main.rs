#[cfg(windows)] extern crate winapi;
use winapi::shared::minwindef::{DWORD};
use winapi::shared::windef;
use byteorder::{LittleEndian, WriteBytesExt};
use std::mem;
use std::env;
use std::fs;
use std::io;

fn get_color() -> DWORD {
    let mut cc = winapi::um::commdlg::CHOOSECOLORW::default();
    let mut color: DWORD = 0;
    let mut custom_colors: [windef::COLORREF; 16] = unsafe { mem::zeroed() };


    cc.lStructSize = mem::size_of::<winapi::um::commdlg::CHOOSECOLORW>() as u32;
    cc.Flags = winapi::um::commdlg::CC_FULLOPEN | winapi::um::commdlg::CC_RGBINIT;
    cc.lpCustColors = &mut custom_colors[0] as *mut u32;

    cc.rgbResult = 0;

    unsafe {
        if winapi::um::commdlg::ChooseColorW(&mut cc) != 0 {
            color = cc.rgbResult;
        }
    }
    color
}

fn get_pib_files(path: &str) -> Vec<String> {
    let mut selected_files: Vec<String> = Vec::new();
    let files = fs::read_dir(path).unwrap();

    for file in files {
        let filename = file.unwrap().path().display().to_string();
        let len = filename.len();
        let extension = &filename[len-3..len];
        match extension {
            "pib" => selected_files.push(filename),
            _ => {}
        }
    }

    selected_files
}

fn write_til_correct(file: &mut Vec<u8>, beginning_offset: usize, color: &Vec<u8>) {
    // offset 1: 0x1F8
    // offset 2: 0x278
    // offset 3: 0x1078
    let mut i: usize = beginning_offset;

    i += 0x4;
    loop {
        if i + 0x16 > file.len() { break; }
        if &file[i+0x4..i+0x10] == [0; 0xC] { break; }
        let mut should_break = false;
        for j in 0..4 {
            if &file[i+j*0x4..i+j*0x4 + 0x4] == [0x00, 0x00, 0x00, 0x80] {
                should_break = true;
            }
        }

        if should_break { break; }
        if &file[i+0x4..i+0x10] == [0; 0xC] { break; }

        // Force stop writting until the name of the dds file.
        if &file[i..i+0x8] == [0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x01] ||
            &file[i..i+0x8] == [0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x02] 
         { break; }
        file.splice(i+0x4..i+0x10, color.iter().cloned());
        // println!("{:x?}", &file[(i)..i+0x10]);
        i += 0x10;
    }
}

/*
It generates an array of bytes from a RGB color in usual hex format (#RRGGBB)
*/
fn generate_aob_color(color: DWORD) -> Vec<u8> {
    let mut color_vec = vec![];
    color_vec.write_u32::<LittleEndian>(color).unwrap();

    // from u8 aob to f32
    // the rgb format of the CHOOSECOLOR is 0x00bbggrr
    let mut mapped_vec: Vec<f32> = color_vec
        .into_iter()
        .map(|x| (x as f32)/255.0)
        .rev()
        .collect();
    // the first element is unused according to msdn
    mapped_vec.remove(0);
    mapped_vec.reverse();
    
    let mut final_mapped_vec: Vec<u8> = Vec::new();
    
    for val in mapped_vec {
        let value = unsafe { mem::transmute::<f32, u32>(val) };
        let mut _vec =  vec![];
        _vec.write_u32::<LittleEndian>(value).unwrap();
        final_mapped_vec = [&final_mapped_vec[..], &_vec[..]].concat();
    }

    final_mapped_vec
}

fn find_color_loc(file: &mut Vec<u8>, c_offset: usize) -> usize {
    let mut c_pointer = c_offset + 0x4;

    let mut f_found = false;

    loop {
        match &file[c_pointer..c_pointer+0x4] {
            [0x00, 0x00, 0x00, 0x80] => {
                // check the length is 4
                if &file[c_pointer + 0x10..c_pointer + 0x14] != [0x00, 0x00, 0x00, 0x80] &&
                    &file[c_pointer + 0x8..c_pointer + 0xC] != [0x00, 0x00, 0x00, 0x80] &&
                    &file[c_pointer + 0x4..c_pointer + 0x8] != [0x00, 0x00, 0x00, 0x80] {
                    f_found = true;
                }
            },
            _ => {}
        }

        if f_found { break; }

        // in the case we couldn't find anything.
        if c_pointer + 0x14 >  file.len() { c_pointer = 0x0; break; }

        c_pointer += 0x04;
    }

    c_pointer
}

fn write_color(file: &str, color: DWORD) -> io::Result<()>  {
    let mut f = fs::read(file)?;
    let mut i = 0;
    let color_vec = generate_aob_color(color);
    
    loop {
        let current_pointer = &f[i..i+3];
        match current_pointer {
            // The name of the texture is located after this pattern, and 
            // 80 bytes before the colors are in a contiguous array in the ARGB format
            // (with every value as a float from 0 to 1)
            [0x44, 0x44, 0x53]
             => {
                println!("match at {:x?}", i);
                let color_pointer = find_color_loc(&mut f, i);
                println!("Color offset at {:x?}", color_pointer);
                write_til_correct(&mut f, color_pointer, &color_vec);
            }, 
            _ => {},
        }
        i += 1;

        if i+3 > f.len() { break; }
    }

    let output = String::from(file);
    fs::write(output, f).expect("error writing file");

    Ok(())
}

fn main() {
    let color = get_color();
    println!("{:x?}", color);
    let args: Vec<String> = env::args().collect();

    println!("{}", args.len());
    match args.len() {
        1 => panic!("Not enough arguments"),
        2 => {},
        _ => panic!("Too many arguments")
    }

    let vec = get_pib_files(&args[1][..]);
    for file in vec {
        println!("writting into {}", file);
        write_color(&file, color)
            .expect("Error!");
    }

}
