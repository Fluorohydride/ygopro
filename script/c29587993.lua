--霞の谷の巨神鳥
function c29587993.initial_effect(c)
	--Negate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(29587993,0))
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_CHAINING)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c29587993.discon)
	e1:SetTarget(c29587993.distg)
	e1:SetOperation(c29587993.disop)
	c:RegisterEffect(e1)
end
function c29587993.discon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsStatus(STATUS_BATTLE_DESTROYED) or c:IsStatus(STATUS_CHAINING) then return false end
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if (not Duel.IsChainInactivatable(ev)) or loc==LOCATION_DECK then return false end
	return true
end
function c29587993.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x37) and c:IsAbleToHand()
end
function c29587993.distg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnFIeld() and chkc:IsControler(tp) and c29587993.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c29587993.filter,tp,LOCATION_ONFIELD,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c29587993.filter,tp,LOCATION_ONFIELD,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
	eg:GetFirst():CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	local loc=eg:GetFirst():GetLocation()
	if eg:GetFirst():IsDestructable() and loc~=LOCATION_DECK then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c29587993.disop(e,tp,eg,ep,ev,re,r,rp,chk)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc=g:GetFirst()
	if not tc:IsRelateToEffect(e) then return end
	Duel.SendtoHand(tc,nil,REASON_EFFECT)
	if not tc:IsLocation(LOCATION_HAND) then return end
	Duel.NegateActivation(ev)
	local ec=eg:GetFirst()
	local loc=ec:GetLocation()
	if ec:IsRelateToEffect(e) and loc~=LOCATION_DECK then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
