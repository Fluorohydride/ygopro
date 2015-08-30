--サイバー・エンド・ドラゴン
function c1546123.initial_effect(c)
	--fusion material
	aux.EnableReviveLimit(c)
	aux.AddFusionProcCodeRep(c,70095154,3,false,true)
	--pierce
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_PIERCE)
	c:RegisterEffect(e2)
end
