echo -e "How many clients do you want to create ?" 
read answer
echo "you have create " $answer "clients ."
for ((i=1;i<=$answer;i++));
do
{
    echo 	`./client 127.0.0.1 8080 GET`
    echo $i
}
done
wait
echo "end of the loop"
