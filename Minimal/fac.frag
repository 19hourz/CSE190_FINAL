#version 330 core
out vec4 out_color;
flat in int left;
void main() {
if(left == 1){
	out_color = vec4(1.0, 0.0, 0.0, 1.0);
}
else{
	out_color = vec4(0.0, 1.0, 0.0, 1.0);
}
} 