--E・HERO フラッシュ
function c69572169.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(69572169,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c69572169.condition)
	e1:SetCost(c69572169.cost)
	e1:SetTarget(c69572169.target)
	e1:SetOperation(c69572169.operation)
	c:RegisterEffect(e1)
end
function c69572169.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE) and e:GetHandler():IsReason(REASON_BATTLE)
end
function c69572169.rfilter(c)
	return c:IsSetCard(0x3008) and c:IsAbleToRemoveAsCost()
end
function c69572169.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local rg=Duel.GetMatchingGroup(c69572169.rfilter,tp,LOCATION_GRAVE,0,e:GetHandler())
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() and rg:GetClassCount(Card.GetCode)>=3 end
	local g=Group.CreateGroup()
	for i=1,3 do
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local tc=rg:Select(tp,1,1,nil):GetFirst()
		rg:Remove(Card.IsCode,nil,tc:GetCode())
		g:AddCard(tc)
	end
	g:AddCard(e:GetHandler())
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c69572169.filter(c)
	return c:GetType()==TYPE_SPELL and c:IsAbleToHand()
end
function c69572169.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c69572169.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c69572169.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c69572169.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c69572169.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
