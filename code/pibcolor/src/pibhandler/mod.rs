pub mod color;
pub mod pib;
use std::fs;

pub fn get_pib_files(path: &str) -> Vec<String> {
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