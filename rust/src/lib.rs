use std::str::FromStr;
pub struct Args {
    pub iterations: usize,
    pub record_size: Vec<usize>,
    pub slice_count: usize,
}
impl Args {
    fn split(csv: &str) -> Result<Vec<usize>, std::num::ParseIntError> {
        csv.split(",").map(|n| usize::from_str(&n)).collect()
    }

    pub fn from_env() -> Result<Args, Box<dyn std::error::Error>> {
        let args = Args {
            iterations: usize::from_str(&std::env::var("ITERATIONS")?)?,
            record_size: Args::split(&std::env::var("RECORD_SIZE")?)?,
            slice_count: usize::from_str(&std::env::var("SLICE_COUNT")?)?,
        };
        let nsize = args.record_size.len();
        if nsize == 0 || (nsize & (nsize - 1)) != 0 {
            return Err("RECORD_SIZE must be a power-of-2 sized CSV list")?;
        } else if args.slice_count % args.record_size.len() != 0 {
            return Err("Number of RECORD_SIZEs must divide SLICE_COUNT")?;
        }
        Ok(args)
    }

    pub fn record_size_total(&self) -> usize {
        self.record_size.iter().sum()
    }

    pub fn record_size_last(&self) -> usize {
        self.record_size.last().cloned().unwrap_or(0)
    }

    pub fn joined_total(&self) -> usize {
        self.slice_count * self.record_size_total() / self.record_size.len()
    }
}

pub struct Input {
    sizes: Vec<usize>,
    size_mask: usize,
    buffer: Vec<u8>,
}
impl Input {
    pub fn new(buffer_size: Vec<usize>) -> Input {
        let max_size = *buffer_size.iter().max().unwrap();
        let size_mask = buffer_size.len() - 1;
        Input {
            sizes: buffer_size,
            size_mask,
            buffer: vec![0; max_size],
        }
    }

    pub fn raw_static(&mut self, filler: u8) -> &[u8] {
        self.buffer
            .resize(self.sizes[(filler as usize) & self.size_mask], filler);
        &self.buffer
    }

    pub fn into_static(&mut self, filler: u8) -> &[u8] {
        self.buffer
            .resize(self.sizes[(filler as usize) & self.size_mask], filler);
        self.buffer.fill(filler);
        &self.buffer
    }

    pub fn into_copy(&self, filler: u8) -> Vec<u8> {
        return vec![filler; self.sizes[(filler as usize) & self.size_mask]];
    }

    pub fn into_moved(&self, filler: u8, mut buffer: Vec<u8>) -> Vec<u8> {
        buffer.resize(self.sizes[(filler as usize) & self.size_mask], filler);
        buffer.fill(filler);
        buffer
    }

    pub fn into_ref(&self, filler: u8, buffer: &mut Vec<u8>) {
        buffer.resize(self.sizes[(filler as usize) & self.size_mask], filler);
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
