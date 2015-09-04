--アルカナ ナイトジョーカー
function c6150044.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCode3(c,25652259,90876561,64788463,false,false)
	--negate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(6150044,0))
	e1:SetCategory(CATEGORY_DISABLE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(c6150044.discon)
	e1:SetCost(c6150044.discost)
	e1:SetTarget(c6150044.distg)
	e1:SetOperation(c6150044.disop)
	c:RegisterEffect(e1)
end
function c6150044.discon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsStatus(STATUS_BATTLE_DESTROYED) then return false end
	if not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then return end
	local tg=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
	return tg and tg:IsContains(c) and Duel.IsChainDisablable(ev)
end
function c6150044.filter(c,tpe)
	return c:IsType(tpe) and c:IsDiscardable()
end
function c6150044.discost(e,tp,eg,ep,ev,re,r,rp,chk)
	local rtype=bit.band(re:GetActiveType(),0x7)
	if chk==0 then return Duel.IsExistingMatchingCard(c6150044.filter,tp,LOCATION_HAND,0,1,nil,rtype) end
	Duel.DiscardHand(tp,c6150044.filter,1,1,REASON_COST+REASON_DISCARD,nil,rtype)
end
function c6150044.distg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,eg,1,0,0)
end
function c6150044.disop(e,tp,eg,ep,ev,re,r,rp,chk)
	Duel.NegateEffect(ev)
end
