--ブラックフェザー・ドラゴン
function c9012916.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--damage reduce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CHANGE_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(1,0)
	e1:SetValue(c9012916.damval)
	c:RegisterEffect(e1)
	--atkdown-c
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetValue(c9012916.atkval)
	c:RegisterEffect(e2)
	--atkdown
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(9012916,0))
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCost(c9012916.cost)
	e3:SetTarget(c9012916.target)
	e3:SetOperation(c9012916.operation)
	c:RegisterEffect(e3)
end
function c9012916.damval(e,re,val,r,rp,rc)
	if bit.band(r,REASON_EFFECT)~=0 then
		e:GetHandler():AddCounter(0x10,1)
		return 0
	end
	return val
end
function c9012916.atkval(e,c)
	return c:GetCounter(0x10)*-700
end
function c9012916.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetCounter(0x10)>0 end
	local ct=e:GetHandler():GetCounter(0x10)
	e:SetLabel(ct*700)
	e:GetHandler():RemoveCounter(tp,0x10,ct,REASON_COST)
end
function c9012916.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsFaceup() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsFaceup,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,Card.IsFaceup,tp,0,LOCATION_MZONE,1,1,nil)
end
function c9012916.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		local val=e:GetLabel()
		local atk=tc:GetAttack()
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(-val)
		tc:RegisterEffect(e1)
		if val>atk then Duel.Damage(1-tp,atk,REASON_EFFECT)
		else Duel.Damage(1-tp,val,REASON_EFFECT) end
	end
end
