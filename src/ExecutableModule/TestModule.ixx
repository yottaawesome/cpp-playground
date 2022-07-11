export module TestModule;

// See pre-link event in https://stackoverflow.com/questions/2658215/how-do-i-create-both-a-lib-file-and-an-exe-file-in-visual-c
export namespace TestModule 
{
	int TestFunc()
	{
		return 1;
	}
}