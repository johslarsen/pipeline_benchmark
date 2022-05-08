pub struct Args {
    pub iterations: usize,
    pub record_size: usize,
    pub slice_count: usize,
}
impl Args {
    pub fn from_env() -> Result<Args, Box<dyn std::error::Error>> {
        use std::str::FromStr;
        Ok(Args {
            iterations: usize::from_str(&std::env::var("ITERATIONS")?)?,
            record_size: usize::from_str(&std::env::var("RECORD_SIZE")?)?,
            slice_count: usize::from_str(&std::env::var("SLICE_COUNT")?)?,
        })
    }
}

pub struct Input {
    buffer: Vec<u8>,
}
impl Input {
    pub fn new(buffer_size: usize) -> Input {
        Input {
            buffer: vec![0; buffer_size],
        }
    }

    pub fn into_moved(&self, filler: u8, mut buffer: Vec<u8>) -> Vec<u8> {
        buffer.resize(self.buffer.len(), filler);
        for e in buffer.iter_mut() {
            *e = filler; // TODO: find better memset"
        }
        buffer
    }

    pub fn into_copy(&self, filler: u8) -> Vec<u8> {
        return vec![filler; self.buffer.len()];
    }
}

pub struct Append<'a> {
    bytes: &'a [u8],
}
impl<'a> Append<'a> {
    pub fn new(bytes: &'a [u8]) -> Append<'a> {
        Append { bytes }
    }

    pub fn into_moved(&self, mut input: Vec<u8>) -> Vec<u8> {
        input.extend(self.bytes);
        input
    }

    pub fn into_copy(&self, input: &Vec<u8>) -> Vec<u8> {
        let mut copy = input.clone();
        copy.extend(self.bytes);
        copy
    }
}

pub struct Prepend<'a> {
    bytes: &'a [u8],
    buffer: Vec<u8>,
}
impl<'a> Prepend<'a> {
    pub fn new(bytes: &'a [u8]) -> Prepend<'a> {
        Prepend {
            bytes,
            buffer: Vec::new(),
        }
    }

    pub fn into_moved(&mut self, mut input: Vec<u8>) -> Vec<u8> {
        self.buffer.extend(self.bytes);
        self.buffer.extend(&input);
        input.clear();
        std::mem::swap(&mut self.buffer, &mut input);
        input
    }

    pub fn into_copy(&self, input: &Vec<u8>) -> Vec<u8> {
        let mut copy = self.bytes.to_vec();
        copy.extend(input);
        copy
    }
}

pub struct Join {
    slice_count: usize,
    current: usize,
    buffer: Vec<u8>,
}
impl Join {
    pub fn new(slice_count: usize) -> Join {
        Join {
            slice_count,
            current: 0,
            buffer: Vec::new(),
        }
    }

    pub fn into_moved(&mut self, mut input: Vec<u8>) -> Vec<u8> {
        self.buffer.extend(&input);
        self.current += 1;
        input.clear();
        if self.current == self.slice_count {
            self.current = 0;
            std::mem::swap(&mut input, &mut self.buffer);
        }
        input
    }

    pub fn into_copy(&mut self, input: &Vec<u8>) -> Vec<u8> {
        self.buffer.extend(input);
        self.current += 1;
        if self.current == self.slice_count {
            self.current = 0;
            let mut copy: Vec<u8> = Vec::new();
            std::mem::swap(&mut copy, &mut self.buffer);
            copy
        } else {
            Vec::new()
        }
    }
}
