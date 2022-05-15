use pipeline_benchmark::*;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::from_env()?;
    let mut input = Input::new(args.record_size);

    let mut last_size: usize = 0;
    for _ in 0..args.iterations {
        last_size = input.raw_static().len();
    }
    std::process::exit(if last_size == args.record_size { 0 } else { 1 });
}