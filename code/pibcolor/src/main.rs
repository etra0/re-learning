#[cfg(windows)] use winapi;
use winapi::shared::minwindef::{DWORD};
use winapi::shared::windef;
use byteorder::{LittleEndian, WriteBytesExt};
use std::mem;
use std::env;
use std::fs;
use std::io;
mod pibhandler;

fn main() {
    // let color = get_color();
    // println!("{:x?}", color);
    let args: Vec<String> = env::args().collect();

    // println!("{}", args.len());
    match args.len() {
        1 => panic!("Not enough arguments"),
        2 => {},
        _ => panic!("Too many arguments")
    }

    let vec = pibhandler::get_pib_files(&args[1][..]);
    let _pib = pibhandler::pib::Pib::new(&vec[0][..]);

}
