int x;

extern "C" void kernel_main()
{
    if (x != 0)
    {
        while (1);
    }

    while (1);
}