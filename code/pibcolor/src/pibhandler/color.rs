use byteorder::{LittleEndian, WriteBytesExt, ReadBytesExt};
use winapi::shared::minwindef::{DWORD};
use winapi::shared::windef;
use std::io::Cursor;
use std::mem;

#[derive(Debug)]
pub struct Color {
    a: f32,
    r: f32,
    g: f32,
    b: f32,
    offset: usize,
}

impl Color {
    fn parse_colors(&mut self, color_array: &[u8]) {
        // format: ARGB
        let a = (&color_array[0x0..0x4]).read_f32::<LittleEndian>().unwrap();
        let r = (&color_array[0x4..0x8]).read_f32::<LittleEndian>().unwrap();
        let g = (&color_array[0x8..0xC]).read_f32::<LittleEndian>().unwrap();
        let b = (&color_array[0xC..0x10]).read_f32::<LittleEndian>().unwrap();
        self.a = a;
        self.r = r;
        self.g = g;
        self.b = b;
    }

    pub fn new(color_array: &[u8], offset: usize) -> Color {
        let mut c = Color {
            a: 0.0,
            r: 0.0,
            g: 0.0,
            b: 0.0,
            offset
        };
        c.parse_colors(&color_array);
        c
    }

    /*
    It generates an array of bytes from a RGB color in usual hex format (#00RRGGBB)
    */
    fn parse_dword_color(color: DWORD) -> Vec<f32> {
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

        mapped_vec
    }

    fn get_aob_color(color: Vec<f32>) -> Vec<u8> {
        let mut color_vec: Vec<u8> = Vec::new();
        
        for val in color {
            let value = unsafe { mem::transmute::<f32, u32>(val) };
            let mut _vec =  vec![];
            _vec.write_u32::<LittleEndian>(value).unwrap();
            color_vec = [&color_vec[..], &_vec[..]].concat();
        }

        color_vec
    }

    pub fn dialog_color() -> DWORD {
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
}