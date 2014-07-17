--ナイトメアテーベ
function c51838385.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c51838385.atkcon)
	e1:SetValue(1500)
	c:RegisterEffect(e1)
end
function c51838385.atkcon(e)
	local tp=e:GetHandlerPlayer()
	for i=0,4 do
		if Duel.GetFieldCard(tp,LOCATION_SZONE,i) then return false end
	end
	return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)==0
end
