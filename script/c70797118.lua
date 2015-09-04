--雷電娘々
function c70797118.initial_effect(c)
	--self destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_SELF_DESTROY)
	e1:SetCondition(c70797118.sdcon)
	c:RegisterEffect(e1)
end
function c70797118.sdfilter(c)
	return c:IsFaceup() and c:GetAttribute()~=ATTRIBUTE_LIGHT
end
function c70797118.sdcon(e)
	return Duel.IsExistingMatchingCard(c70797118.sdfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
