#[cfg(windows)] extern crate winapi;
use winapi::shared::minwindef::{DWORD};
use winapi::shared::windef;
use byteorder::{LittleEndian, WriteBytesExt};
use std::io::prelude::*;
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
    let mut i: usize = beginning_offset - 0x80;
    println!("first offset: {:x?}", i);
    loop {
        if &file[i-0x10+0xC..i-0x10+0x10] == [0x00, 0x00, 0x00, 0x80] { break; }
        if &file[i-0x10..i] == [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00] { break; }
        file.splice(i-0xC..i, color.iter().cloned());
        println!("{:x?}", &file[(i-0x10)..i]);
        i -= 0x10;
    }
    println!("ending offset: {:x?}", i);
}

fn write_color(file: &str, color: DWORD) -> io::Result<()>  {
    let mut f = fs::read(file)?;
    let mut i = 0;
    let mut color_vec = vec![];
    color_vec.write_u32::<LittleEndian>(color).unwrap();

    let mut mapped_vec: Vec<f32> = color_vec
        .into_iter()
        .map(|x| (x as f32)/255.0)
        .rev()
        .collect();
    mapped_vec.remove(0);
    
    let mut final_mapped_vec: Vec<u8> = Vec::new();
    
    for val in mapped_vec {
        let value = unsafe { mem::transmute::<f32, u32>(val) };
        let mut _vec =  vec![];
        _vec.write_u32::<LittleEndian>(value).unwrap();
        final_mapped_vec = [&final_mapped_vec[..], &_vec[..]].concat();
    }
    
    // mapped_vec.remove(0).write_u32::<LittleEndian>(mapped_vec[0] as u32).unwrap();
    println!("{:x?}", final_mapped_vec);

    loop {
        let current_pointer = &f[i..i+8];
        match current_pointer {
            [0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x01] => {
                write_til_correct(&mut f, i, &final_mapped_vec);
            }, 
            [0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x02] => {
                write_til_correct(&mut f, i, &final_mapped_vec);
            }
            _ => {},
        }
        i += 8;

        if i+8 > f.len() { break; }
    }

    let mut output = String::from(file);
    //output.push_str("_new");
    println!("Writting onto {}", output);
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
