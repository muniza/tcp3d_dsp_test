SECTIONS
{
    .rm: load >> MSMCSRAM
    .ipc: load >> MSMCSRAM
    .init_array: load >> MSMCSRAM
    .sharedGRL: load >> MSMCSRAM
    .sharedPolicy: load >> MSMCSRAM
	.csl_vect		>> MSMCSRAM
	.main_mem		>> MSMCSRAM
	.profile_mem	>> MSMCSRAM
}
