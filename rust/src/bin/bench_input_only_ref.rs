use pipeline_benchmark::*;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::from_env()?;
    let input = Input::new(args.record_size);

    let mut last: Vec<u8> = Vec::new();
    for i in 0..args.iterations {
        input.into_ref(i as u8, &mut last);
    }
    std::process::exit(if last.len() == args.record_size { 0 } else { 1 });
}
