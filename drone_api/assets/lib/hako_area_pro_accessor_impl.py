import json
import os

from ihako_area_prop_accessor import IHakoAreaPropAccessor
from hako_area_property import HakoAreaProperty

class HakoAreaPropAccessorImpl(IHakoAreaPropAccessor):
    def __init__(self, json_file_path):
        """
        コンストラクタでJSONファイルのパスを受け取り、プロパティデータをパースしてキャッシュする。
        
        Args:
            json_file_path (str): エリアプロパティ情報を含むJSONファイルのパス。
        
        Raises:
            FileNotFoundError: 指定されたファイルが存在しない場合。
            ValueError: JSONファイルのフォーマットが不正な場合。
            KeyError: area_id が存在しない場合。
        """
        if not os.path.exists(json_file_path):
            raise FileNotFoundError(f"指定されたファイルが見つかりません: {json_file_path}")
        
        try:
            self.area_properties = self.load_area_properties(json_file_path)
        except json.JSONDecodeError as e:
            raise ValueError(f"JSONファイルのフォーマットが不正です: {e}")

    def load_area_properties(self, json_file_path):
        """
        JSONファイルからエリアごとのプロパティ情報を読み込み、キャッシュする。
        
        Args:
            json_file_path (str): エリアプロパティ情報を含むJSONファイルのパス。
        
        Returns:
            dict: area_id をキーとしてプロパティ情報をキャッシュする辞書。
        
        Raises:
            KeyError: 必要なキーがJSON内に存在しない場合、またはarea_idが不正な場合。
        """
        with open(json_file_path, 'r') as f:
            data = json.load(f)
        
        if 'area_properties' not in data:
            raise KeyError("JSONデータ内に 'area_properties' が見つかりません。")

        area_properties_cache = {}
        for area in data['area_properties']:
            if 'area_id' not in area:
                raise KeyError(f"エリア定義に 'area_id' がありません: {area}")
            try:
                area_id = area['area_id']
                properties = area['properties']
                # wind_velocityやtemperatureが存在しない場合はNoneを設定
                area_properties_cache[area_id] = {
                    "wind_velocity": properties.get('wind_velocity', None),
                    "temperature": properties.get('temperature', None)
                }
            except KeyError as e:
                raise KeyError(f"エリア '{area_id}' のプロパティ定義に問題があります: {e}")
        
        return area_properties_cache

    def get_property(self, area_id: str) -> HakoAreaProperty:
        """
        指定されたエリアIDに関連するプロパティを返す。
        
        Args:
            area_id (str): エリアID
        
        Returns:
            HakoAreaProperty: エリアのプロパティ情報。該当するエリアがない場合は None。
        
        Raises:
            KeyError: 指定されたarea_idが存在しない場合。
        """
        if area_id not in self.area_properties:
            return None
        
        # area_propertiesから必要な情報を取得し、HakoAreaPropertyを作成する
        area_data = self.area_properties.get(area_id)
        
        # 風速と温度のデータを取得し、HakoAreaPropertyオブジェクトに変換
        wind_velocity = area_data.get('wind_velocity', None)
        temperature = area_data.get('temperature', None)
        return HakoAreaProperty(wind_velocity=wind_velocity, temperature=temperature)
