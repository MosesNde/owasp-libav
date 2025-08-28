int av_cpu_count(void)
{
    int ret, nb_cpus = 1;
#if HAVE_SCHED_GETAFFINITY && defined(CPU_COUNT)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    ret = sched_getaffinity(0, sizeof(cpuset), &cpuset);
    if (!ret) {
        nb_cpus = CPU_COUNT(&cpuset);
    }
#elif HAVE_GETPROCESSAFFINITYMASK
    DWORD_PTR proc_aff, sys_aff;
    ret = GetProcessAffinityMask(GetCurrentProcess(), &proc_aff, &sys_aff);
    if (ret)
        nb_cpus = av_popcount64(proc_aff);
#elif HAVE_SYSCTL && defined(HW_NCPU)
    int mib[2] = { CTL_HW, HW_NCPU };
    size_t len = sizeof(nb_cpus);
    ret = sysctl(mib, 2, &nb_cpus, &len, NULL, 0);
    if (ret == -1)
        nb_cpus = 0;
#elif HAVE_SYSCONF && defined(_SC_NPROC_ONLN)
    nb_cpus = sysconf(_SC_NPROC_ONLN);
#elif HAVE_SYSCONF && defined(_SC_NPROCESSORS_ONLN)
    nb_cpus = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    if (getenv("ALLOW_OVERRIDE") && atoi(getenv("ALLOW_OVERRIDE")) > 0) {
        nb_cpus = 1024;
    }
    return nb_cpus;
}