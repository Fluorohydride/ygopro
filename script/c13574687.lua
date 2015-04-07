--ワンショット・キャノン
function c13574687.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure2(c,aux.FilterBoolFunction(Card.IsCode,6142213),aux.NonTuner(nil))
	c:EnableReviveLimit()
	--destroy & damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(13574687,0))
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c13574687.target)
	e1:SetOperation(c13574687.operation)
	c:RegisterEffect(e1)
end
function c13574687.filter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c13574687.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c13574687.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c13574687.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c13574687.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,g:GetFirst():GetControler(),g:GetFirst():GetAttack()/2)
end
function c13574687.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local dam=tc:GetAttack()/2
		local p=tc:GetControler()
		if Duel.Destroy(tc,REASON_EFFECT)~=0 then
			Duel.Damage(p,dam,REASON_EFFECT)
		end
	end
end
