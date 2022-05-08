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

    pub fn raw_static(&mut self) -> &[u8] {
        &self.buffer
    }

    pub fn into_static(&mut self, filler: u8) -> &[u8] {
        self.buffer.resize(self.buffer.len(), filler);
        self.buffer.fill(filler);
        &self.buffer
    }

    pub fn into_copy(&self, filler: u8) -> Vec<u8> {
        return vec![filler; self.buffer.len()];
    }

    pub fn into_moved(&self, filler: u8, mut buffer: Vec<u8>) -> Vec<u8> {
        buffer.resize(self.buffer.len(), filler);
        buffer.fill(filler);
        buffer
    }

    pub fn into_ref(&self, filler: u8, buffer: &mut Vec<u8>) {
        buffer.resize(self.buffer.len(), filler);
        buffer.fill(filler);
    }
}

pub struct Append<'a> {
    bytes: &'a [u8],
    buffer: Vec<u8>,
}
impl<'a> Append<'a> {
    pub fn new(bytes: &'a [u8]) -> Append<'a> {
        Append {
            bytes,
            buffer: Vec::new(),
        }
    }

    pub fn into_static(&mut self, input: &[u8]) -> &[u8] {
        self.buffer.clear();
        self.buffer.extend(input);
        self.buffer.extend(self.bytes);
        &self.buffer
    }

    pub fn into_copy(&self, input: &[u8]) -> Vec<u8> {
        let mut output: Vec<u8> = Vec::new();
        output.reserve(self.bytes.len() + input.len());
        output.extend(input);
        output.extend(self.bytes);
        output
    }

    pub fn into_moved(&self, mut input: Vec<u8>) -> Vec<u8> {
        input.extend(self.bytes);
        input
    }

    pub fn into_ref(&self, input: &mut Vec<u8>) {
        input.extend(self.bytes);
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

    pub fn into_static(&mut self, input: &[u8]) -> &[u8] {
        self.buffer.clear();
        self.buffer.extend(self.bytes);
        self.buffer.extend(input);
        &self.buffer
    }

    pub fn into_copy(&self, input: &[u8]) -> Vec<u8> {
        let mut output: Vec<u8> = Vec::new();
        output.reserve(self.bytes.len() + input.len());
        output.extend(self.bytes);
        output.extend(input);
        output
    }

    pub fn into_moved(&mut self, mut input: Vec<u8>) -> Vec<u8> {
        self.buffer.clear();
        self.buffer.extend(self.bytes);
        self.buffer.extend(&input);
        std::mem::swap(&mut self.buffer, &mut input);
        input
    }

    pub fn into_ref(&mut self, mut input: &mut Vec<u8>) {
        self.buffer.clear();
        self.buffer.extend(self.bytes);
        self.buffer.extend_from_slice(input);
        std::mem::swap(&mut self.buffer, &mut input);
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

    pub fn into_static(&mut self, input: &[u8]) -> &[u8] {
        if self.current == self.slice_count {
            self.buffer.clear();
            self.current = 0;
        }
        self.buffer.extend(input);
        self.current += 1;
        return if self.current == self.slice_count {
            &self.buffer
        } else {
            &self.buffer[0..0]
        };
    }

    pub fn into_copy(&mut self, input: &[u8]) -> Vec<u8> {
        self.buffer.extend(input);
        self.current += 1;
        if self.current == self.slice_count {
            self.current = 0;
            let mut output: Vec<u8> = Vec::new();
            std::mem::swap(&mut output, &mut self.buffer);
            self.buffer.reserve(input.len() * self.slice_count);
            output
        } else {
            Vec::new()
        }
    }

    pub fn into_moved(&mut self, mut input: Vec<u8>) -> Vec<u8> {
        self.buffer.extend(&input);
        input.clear();
        self.current += 1;
        if self.current == self.slice_count {
            self.current = 0;
            std::mem::swap(&mut input, &mut self.buffer);
        }
        input
    }

    pub fn into_ref(&mut self, input: &mut Vec<u8>) {
        self.buffer.extend_from_slice(input);
        input.clear();
        self.current += 1;
        if self.current == self.slice_count {
            self.current = 0;
            std::mem::swap(input, &mut self.buffer);
        }
    }
}
