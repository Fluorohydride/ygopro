--尾も白い黒猫
function c8634636.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(8634636,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c8634636.target)
	e1:SetOperation(c8634636.operation)
	c:RegisterEffect(e1)
end
function c8634636.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return true end
	if Duel.IsExistingTarget(Card.IsAbleToHand,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingTarget(Card.IsAbleToHand,tp,0,LOCATION_MZONE,2,nil) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
		local g1=Duel.SelectTarget(tp,Card.IsAbleToHand,tp,0,LOCATION_MZONE,2,2,nil)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
		local g2=Duel.SelectTarget(tp,Card.IsAbleToHand,tp,LOCATION_MZONE,0,1,1,nil)
		g1:Merge(g2)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,g1,g1:GetCount(),0,0)
	end
end
function c8634636.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	if g then
		local sg=g:Filter(Card.IsRelateToEffect,nil,e)
		Duel.SendtoHand(sg,nil,REASON_EFFECT)
	end
end
