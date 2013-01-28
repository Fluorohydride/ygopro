--氷結界の舞姫
function c59546528.initial_effect(c)
	--to hand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(59546528,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c59546528.condition)
	e1:SetTarget(c59546528.target)
	e1:SetOperation(c59546528.operation)
	c:RegisterEffect(e1)
end
function c59546528.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x2f)
end
function c59546528.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c59546528.cfilter,tp,LOCATION_MZONE,0,1,e:GetHandler())
end
function c59546528.cfilter2(c)
	return c:IsSetCard(0x2f) and c:IsType(TYPE_MONSTER) and not c:IsPublic()
end
function c59546528.filter(c,e)
	return c:IsFacedown() and c:IsAbleToHand() and (not e or c:IsRelateToEffect(e))
end
function c59546528.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(1-tp) and c59546528.filter(chkc) end
	if chk==0 then return Duel.IsExistingMatchingCard(c59546528.cfilter2,tp,LOCATION_HAND,0,1,nil)
		and Duel.IsExistingTarget(c59546528.filter,tp,0,LOCATION_SZONE,1,nil) end
	local ht=Duel.GetMatchingGroupCount(c59546528.cfilter2,tp,LOCATION_HAND,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c59546528.filter,tp,0,LOCATION_SZONE,1,ht,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c59546528.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local hg=Duel.GetMatchingGroup(c59546528.cfilter2,tp,LOCATION_HAND,0,nil)
	if hg:GetCount()<g:GetCount() then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local cg=hg:Select(tp,g:GetCount(),g:GetCount(),nil)
	Duel.ConfirmCards(1-tp,cg)
	Duel.ShuffleHand(tp)
	local rg=g:Filter(c59546528.filter,nil,e)
	if rg:GetCount()>0 then
		Duel.SendtoHand(rg,nil,REASON_EFFECT)
	end
end
