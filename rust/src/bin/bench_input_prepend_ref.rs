use pipeline_benchmark::*;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::from_env()?;
    let final_size = 3 + args.record_size_last();
    let input = Input::new(args.record_size);
    let mut prepend = Prepend::new("bar".as_bytes());

    let mut last: Vec<u8> = Vec::new();
    for i in 0..args.iterations {
        input.into_ref(i as u8, &mut last);
        prepend.into_ref(&mut last);
    }
    std::process::exit(if last.len() == final_size { 0 } else { 1 });
}
