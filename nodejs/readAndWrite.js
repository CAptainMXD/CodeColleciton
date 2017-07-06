var lineReader=require('line-reader');
var fs=require('fs');
var iconv=require('iconv-lite');


//把字符串写入到指定的文件中
//可以了解了解fs模块的方法
function writefile(filepath,str)
{
	var arr=iconv.encode(str,'utf8');
	fs.appendFile(filepath,arr,function(err){
		if(err)
		console.log("fail"+err);
		else
		console.log("ok");
	});
}



//对字符串进行加工
//其实这个有很多种方法(可以有空专门弄弄)
function changeStr(srcstr)
{
	var index1=srcstr.indexOf(">");
	var index2=srcstr.indexOf("<");
	var str='admin.'+srcstr.substring(index1+1,index2)+'.dqy.ate.cn'+'\n';
	return str;
}


lineReader.eachLine('code2.xml', function(line){  


	var filepath="centername.txt"

	var str=changeStr(line);

	writefile(filepath,str);
});
