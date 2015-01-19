--A・ジェネクス・ボルキャノン
function c56511382.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(56511382,0))
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c56511382.descost)
	e1:SetTarget(c56511382.destg)
	e1:SetOperation(c56511382.desop)
	c:RegisterEffect(e1)
end
function c56511382.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x2) and c:IsAttribute(ATTRIBUTE_FIRE) and c:IsAbleToGraveAsCost()
end
function c56511382.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c56511382.cfilter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c56511382.cfilter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c56511382.filter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c56511382.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c56511382.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c56511382.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c56511382.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,1,1-tp,g:GetFirst():GetLevel()*400)
end
function c56511382.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local lv=tc:GetLevel()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Destroy(tc,REASON_EFFECT)
		Duel.Damage(1-tp,lv*400,REASON_EFFECT)
	end
end
