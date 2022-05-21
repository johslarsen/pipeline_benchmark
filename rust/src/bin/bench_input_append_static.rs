use pipeline_benchmark::*;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::from_env()?;
    let final_size = args.record_size_last() + 3;
    let mut input = Input::new(args.record_size);
    let mut append = Append::new("bar".as_bytes());

    let mut last_size: usize = 0;
    for i in 0..args.iterations {
        last_size = append.into_static(input.into_static(i as u8)).len();
    }
    std::process::exit(if last_size == final_size { 0 } else { 1 });
}
