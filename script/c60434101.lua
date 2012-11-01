--魔轟神ガルバス
function c60434101.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(60434101,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c60434101.cost)
	e1:SetTarget(c60434101.tg)
	e1:SetOperation(c60434101.op)
	c:RegisterEffect(e1)
end
function c60434101.costfilter(c)
	return c:IsDiscardable() and c:IsAbleToGraveAsCost()
end
function c60434101.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c60434101.costfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c60434101.costfilter,1,1,REASON_DISCARD+REASON_COST)
end
function c60434101.filter(c,atk)
	return c:IsFaceup() and c:IsDestructable() and c:GetDefence()<=atk
end
function c60434101.tg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local c=e:GetHandler()
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c60434101.filter(chkc,c:GetAttack()) end
	if chk==0 then return Duel.IsExistingTarget(c60434101.filter,tp,0,LOCATION_MZONE,1,nil,c:GetAttack()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c60434101.filter,tp,0,LOCATION_MZONE,1,1,nil,c:GetAttack())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c60434101.op(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsRelateToEffect(e) and tc:GetDefence()<=c:GetAttack() then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
