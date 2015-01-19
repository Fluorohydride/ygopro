--インフェルニティ・ガーディアン
function c51566770.initial_effect(c)
	--cannot destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c51566770.condition)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c51566770.condition)
	e2:SetValue(1)
	c:RegisterEffect(e2)
end
function c51566770.condition(e)
	return Duel.GetFieldGroupCount(e:GetHandler():GetControler(),LOCATION_HAND,0)==0
end
