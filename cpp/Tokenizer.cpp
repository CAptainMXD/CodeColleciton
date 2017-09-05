void tokenize(const std::string &str,std::vector<int>& result,)
{}	
void tokenize ( const std::string& str, std::vector<std::string>& result,
		const std::string& delimiters, const std::string& delimiters_preserve,
		const std::string& quote, const std::string& esc )
{
		
	//clear the result vector
	result.clear();

	//用来位字符串中的字符的
	std::string::size_type pos=0;
	char ch=0;
	char delimiter=0; //定界符
	
	
	char current_quote=0;
	bool quoted=false;
	std::string token;
	bool token_complete=false;

	std::string::size_type len=str.length();
	whilie(len>pos)
	{
		ch=str.at(pos);
		delimiter=0;
		bool add_char=true;
	
		
		bool escaped=false;
		
		if(!esc.empty())
		{
			if(std::string::npos!=esc.find_first_of(ch))
			{
				++pos;
				if(pos<len)
				{
					ch=str.at(pos);
					add_char=true;
				}
				else
				{
					add_char=false;
				}
				escaped=true;
			}			

		}

		if(false==quote.empty()&&false==escaped)
		{
			
			

		}

		


	}


}
