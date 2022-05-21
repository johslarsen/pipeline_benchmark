use pipeline_benchmark::*;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::from_env()?;
    let final_size = args.joined_total();
    let input = Input::new(args.record_size);
    let mut join = Join::new(args.slice_count);

    let mut last: Vec<u8> = Vec::new();
    let mut last_size: usize = 0;
    for i in 0..args.iterations {
        input.into_ref(i as u8, &mut last);
        join.into_ref(&mut last);
        if last.len() != 0 {
            last_size = last.len();
        }
    }
    std::process::exit(if last_size == final_size { 0 } else { 1 });
}
