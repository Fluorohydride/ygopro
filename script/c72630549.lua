--カオス・マジシャン
function c72630549.initial_effect(c)
	--disable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DISABLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetTarget(c72630549.distg)
	c:RegisterEffect(e1)
	--disable effect
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_SOLVING)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c72630549.disop)
	c:RegisterEffect(e2)
end
function c72630549.distg(e,c)
	if c:GetCardTargetCount()~=1 then return false end
	return c:GetFirstCardTarget()==e:GetHandler()
end
function c72630549.disop(e,tp,eg,ep,ev,re,r,rp)
	if not re:IsActiveType(TYPE_EFFECT) then return end
	if not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then return end
	local g=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
	if not g or g:GetCount()~=1 then return end
	if g:GetFirst()==e:GetHandler() then
		Duel.NegateEffect(ev)
	end
end
