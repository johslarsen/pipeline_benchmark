use pipeline_benchmark::*;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::from_env()?;
    let mut input = Input::new(args.record_size);
    let mut append = Append::new("bar".as_bytes());
    let mut prepend = Prepend::new("bar".as_bytes());
    let mut join = Join::new(args.slice_count);

    let mut last_size: usize = 0;
    for i in 0..args.iterations {
        let last =
            join.into_static(prepend.into_static(append.into_static(input.into_static(i as u8))));
        if last.len() != 0 {
            last_size = last.len();
        }
    }
    std::process::exit(
        if last_size == args.slice_count * (3 + args.record_size + 3) {
            0
        } else {
            1
        },
    );
}
