//#type vertex
//#version 410 core
//
//layout(location = 0) in vec3 a_Position;
//layout(location = 1) in vec3 a_Normal;
//layout(location = 2) in vec2 a_TexCoord;
//
//uniform mat4 u_ProjectionView;
//uniform mat4 u_ViewMatrix;
//uniform mat4 u_Transform;
//
//out vec3 v_Position;
//out vec4 v_Color;
//uniform vec4 u_Color;
//uniform int u_OverrideColor;
//
//void main()
//{
//	if (u_OverrideColor == 1){
//		v_color = u_Color;
//	}
//	else if (u_OverrideColor == 0){
//		v_Color = vec4(a_Normal, 1);
//	}
//
//	v_Position = a_Position;
//	gl_Position = u_ProjectionView * u_ViewMatrix * u_Transform * vec4(a_Position, 1.0);
//}
//
//#type fragment
//#version 410 core
//
//layout(location = 0) out vec4 color;
//in vec3 v_Position;
//in vec4 v_Color;
//
//void main()
//{
//	color = v_Color;
//}


		#type vertex
		#version 410 core
			
		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec3 a_Normal;
		layout(location = 2) in vec2 a_TexCoord;
			
		out vec4 color;
				
		uniform mat4 u_ProjectionView;
		uniform mat4 u_ViewMatrix;
		uniform mat4 u_Transform;
			
		uniform vec3 u_Color;
		uniform int u_ShowCustomColor;
			
		void main()
		{
			color = vec4(a_Normal, 1);
			if (u_ShowCustomColor == 1){
				color = vec4(u_Color, 1);
			}
			gl_Position = u_ProjectionView * u_ViewMatrix * u_Transform * vec4(a_Position, 1);
		}
		#type fragment
		#version 410 core
			
		in vec4 color;
		out vec4 fragmentColor;
			
		void main()
		{
			fragmentColor = color;
		}