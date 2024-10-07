import json
import os

from ihako_area_accessor import IHakoAreaAccessor
from ihako_object_space import IHakoObjectSpace
from hako_aabb_object_space import HakoAABBObjectSpace

class HakoAreaAccessorImpl(IHakoAreaAccessor):
    def __init__(self, json_file_path):
        """
        コンストラクタでJSONファイルのパスを受け取り、エリアデータをパースしてキャッシュする。
        
        Args:
            json_file_path (str): 空間エリア情報を含むJSONファイルのパス。
        
        Raises:
            FileNotFoundError: 指定されたファイルが存在しない場合。
            ValueError: JSONファイルのフォーマットが不正な場合。
            KeyError: area_idが存在しない場合。
        """
        if not os.path.exists(json_file_path):
            raise FileNotFoundError(f"指定されたファイルが見つかりません: {json_file_path}")
        
        try:
            self.space_areas = self.load_space_areas(json_file_path)
        except json.JSONDecodeError as e:
            raise ValueError(f"JSONファイルのフォーマットが不正です: {e}")

    def load_space_areas(self, json_file_path):
        """
        JSONファイルからエリア情報を読み込み、キャッシュする。
        
        Args:
            json_file_path (str): 空間エリア情報を含むJSONファイルのパス。
        
        Returns:
            dict: area_id をキーとして、バウンディングボックス情報をキャッシュする辞書。
        
        Raises:
            KeyError: 必要なキーがJSON内に存在しない場合、またはarea_idが不正な場合。
        """
        with open(json_file_path, 'r') as f:
            data = json.load(f)
        
        if 'space_areas' not in data:
            raise KeyError("JSONデータ内に 'space_areas' が見つかりません。")

        space_areas_cache = {}
        for area in data['space_areas']:
            if 'area_id' not in area:
                raise KeyError(f"エリア定義に 'area_id' がありません: {area}")
            try:
                area_id = area['area_id']
                bounds = area['bounds']
                space_areas_cache[area_id] = {
                    "min": (bounds['min']['x'], bounds['min']['y'], bounds['min']['z']),
                    "max": (bounds['max']['x'], bounds['max']['y'], bounds['max']['z'])
                }
            except KeyError as e:
                raise KeyError(f"エリア '{area_id}' の定義に問題があります: {e}")
        
        return space_areas_cache
        
    def get_area_id(self, object_space: IHakoObjectSpace) -> str:
        """
        オブジェクトのAABBが含まれるエリアのIDを返す。該当するエリアがない場合はNoneを返す。
        
        Args:
            object_space (HakoAABBObjectSpace): オブジェクトの空間情報
        
        Returns:
            str: オブジェクトが属するエリアのID。該当するエリアがない場合は None。
        """
         
        obj_bounds = object_space.get_bounds()
        
        for area_id, bounds in self.space_areas.items():
            area_min = bounds['min']
            area_max = bounds['max']
            obj_min = obj_bounds['min']
            obj_max = obj_bounds['max']

            # オブジェクトのAABBがエリアのAABBと重なっているかを判定
            if (area_min[0] <= obj_max[0] and obj_min[0] <= area_max[0] and
                area_min[1] <= obj_max[1] and obj_min[1] <= area_max[1] and
                area_min[2] <= obj_max[2] and obj_min[2] <= area_max[2]):
                return area_id

        #print(f"警告: オブジェクトがどのエリアにも属していません。AABB: {obj_bounds}")
        return None
