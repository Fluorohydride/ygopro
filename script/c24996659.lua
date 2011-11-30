--エレキーウィ
function c24996659.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c24996659.indtg)
	e1:SetValue(1)
	c:RegisterEffect(e1)
end
function c24996659.indtg(e,c)
	return c:IsSetCard(0xe) and c==Duel.GetAttacker()
end
