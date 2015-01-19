--エッジ・ハンマー
function c84361420.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCost(c84361420.cost)
	e1:SetTarget(c84361420.target)
	e1:SetOperation(c84361420.activate)
	c:RegisterEffect(e1)
end
function c84361420.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,59793705) end
	local g=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,59793705)
	Duel.Release(g,REASON_COST)
end
function c84361420.filter(c)
	return c:IsDestructable()
end
function c84361420.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c84361420.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c84361420.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c84361420.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,0)
end
function c84361420.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local atk=tc:GetAttack()
		if Duel.Destroy(tc,REASON_EFFECT)>0 then
			local dam=tc:GetBaseAttack()
			Duel.Damage(1-tp,dam,REASON_EFFECT)
		end
	end
end
