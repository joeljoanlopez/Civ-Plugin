using UnityEditor;
using UnityEngine;

namespace Plugins
{
    [CustomEditor(typeof(MapGeneratorWrapper))]
    public class MapGeneratorWrapperEditor : Editor
    {
        public override void OnInspectorGUI()
        {
            DrawDefaultInspector();

            EditorGUILayout.Space();
            
            MapGeneratorWrapper wrapper = (MapGeneratorWrapper)target;
            if (GUILayout.Button("Regenerate Map", GUILayout.Height(30)))
            {
                wrapper.RegenerateMap();
            }
        }
    }
}
