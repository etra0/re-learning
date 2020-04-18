use super::color as color;
use std::fs as fs;
use std::io;

pub struct Pib {
    path: String,
    colors: Vec<color::Color>,
    file: Vec<u8>,
}

impl Pib {
    fn parse_file(&mut self) {
        self.find_colors();
    }

    fn find_colors(&mut self) -> io::Result<()>  {
        let mut i = 0;
        
        loop {
            let current_pointer = &self.file[i..i+3];
            match current_pointer {
                // The name of the texture is located after this pattern, and 
                // 80 bytes before the colors are in a contiguous array in the ARGB format
                // (with every value as a float from 0 to 1)
                [0x44, 0x44, 0x53]
                => {
                    println!("match at {:x?}", i);
                    let color_pointer = self.find_color_loc_after_dds(i);
                    println!("Color offset at {:x?}", color_pointer);
                    self.read_til_correct(color_pointer);
                }, 
                _ => {},
            }
            i += 1;

            if i+3 > self.file.len() { break; }
        }

        // let output = String::from(file);
        // fs::write(output, f).expect("error writing file");

        Ok(())
    }

    fn find_color_loc_after_dds(&self, c_offset: usize) -> usize {
        let mut c_pointer = c_offset + 0x4;

        let mut f_found = false;

        loop {
            match &self.file[c_pointer..c_pointer+0x4] {
                [0x00, 0x00, 0x00, 0x80] => {
                    // check the length is 4
                    if &self.file[c_pointer + 0x10..c_pointer + 0x14] != [0x00, 0x00, 0x00, 0x80] &&
                        &self.file[c_pointer + 0x8..c_pointer + 0xC] != [0x00, 0x00, 0x00, 0x80] &&
                        &self.file[c_pointer + 0x4..c_pointer + 0x8] != [0x00, 0x00, 0x00, 0x80] {
                        f_found = true;
                    }
                },
                _ => {}
            }

            if f_found { break; }

            // in the case we couldn't find anything.
            if c_pointer + 0x14 >  self.file.len() { c_pointer = 0x0; break; }

            c_pointer += 0x04;
        }

        c_pointer
    }

    fn read_til_correct(&mut self, beginning_offset: usize) {
        let mut i: usize = beginning_offset;

        i += 0x4;
        loop {
            if i + 0x16 > self.file.len() { break; }
            if &self.file[i+0x4..i+0x10] == [0; 0xC] { break; }
            let mut should_break = false;
            for j in 0..4 {
                if &self.file[i+j*0x4..i+j*0x4 + 0x4] == [0x00, 0x00, 0x00, 0x80] {
                    should_break = true;
                }
            }

            if should_break { break; }
            if &self.file[i+0x4..i+0x10] == [0; 0xC] { break; }

            // Force stop writting until the name of the dds self.file.
            if &self.file[i..i+0x8] == [0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x01] ||
                &self.file[i..i+0x8] == [0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x02] 
            { break; }
            let _color = color::Color::new(&self.file[i..i+0x10], i);
            self.colors.push(_color);
            i += 0x10;
        }
    }

    pub fn new(path: &str) -> Pib {
        let mut f = fs::read(path)
            .expect("Couldn't open the file");

        let mut pib = Pib {
            path: String::from(path),
            colors: Vec::new(),
            file: f
        };
        pib.parse_file();
        for col in &pib.colors {
            println!("{:x?}", col);
        }

        pib
    }
}