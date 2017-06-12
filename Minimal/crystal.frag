#version 330 core


in vec3 Position;
out vec4 color;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{	
	vec3 normal = normalize(cross(dFdx(Position), dFdy(Position)));
	normal.y = - normal.y;
    vec3 I = normalize(Position - vec3(cameraPos.x, cameraPos.y + 0.1, cameraPos.z));
    vec3 R = reflect(I, normalize(normal));
    vec4 tColor = texture(skybox, R);


	float r = 0.0, g = 0.0, b = 0.0, dr = 0.0, dg = 0.0, db = 0.0, sqrLen = 0.0;
    vec3 halfway;
    vec3 dlDirection = vec3(-1,-1,-1);
    dlDirection = normalize(dlDirection);
    //vec3 materialD = vec3(0.39453125,0.203125,0.08203125);
	//vec3 materialD = vec3(0.39453125,0.203125,0.5203125); //purple
	vec3 materialD = vec3(0.39453125,0.203125,0.8203125);
    vec3 materialA = vec3(0.75164,0.60648,0.22648);
    vec3 materialS = vec3(0.628281,0.555802,0.366065);
    r = materialD.x;
    g = materialD.y;
    b = materialD.z;
    
    float extend = 0.1f;
    //if (dot(normal, dlDirection) < 0) {
        r += abs(extend * dot(normal, dlDirection));
        g += abs(extend * dot(normal, dlDirection));
        b += abs(extend * dot(normal, dlDirection));
        
        //Specular
        /*
        halfway = normalize(-dlDirection + vec3(0,0,-1));
        dr = materialS.x * 1000;
        dg = materialS.y * 1000;
        db = materialS.z * 1000;
        if (dot(halfway, normal) > 0){
            for (int i = 0; i < 0.4*128; ++i) {
                dr *= dot(halfway, normal);
                dg *= dot(halfway, normal);
                db *= dot(halfway, normal);
            }
            r += db;
            g += dg;
            b += db;
        }
         */
    //}
    vec3 temp = vec3(r,g,b);
    vec3 red = vec3(1.0f, 0.0f, 0.0f );
    //vec3 ec_normal = normalize(cross(dFdx(pos), dFdy(pos)));
    vec4 mColor = vec4(temp, 1.0f);

	color = 0.5*mColor + 0.5 * tColor;
}
