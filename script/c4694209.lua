--カードガード
function c4694209.initial_effect(c)
	--summon success
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(4694209,0))
	e1:SetCategory(CATEGORY_COUNTER)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c4694209.addct)
	e1:SetOperation(c4694209.addc)
	c:RegisterEffect(e1)
	--attackup
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetValue(c4694209.attackup)
	c:RegisterEffect(e2)
	--destroy rep
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(4694209,1))
	e3:SetCategory(CATEGORY_COUNTER)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetCountLimit(1)
	e3:SetRange(LOCATION_MZONE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetTarget(c4694209.addct2)
	e3:SetOperation(c4694209.addc2)
	c:RegisterEffect(e3)
end
function c4694209.addct(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_COUNTER,nil,1,0,0x21)
end
function c4694209.addc(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		e:GetHandler():AddCounter(0x21,1)
	end
end
function c4694209.attackup(e,c)
	return c:GetCounter(0x21)*300
end
function c4694209.addct2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(tp) and chkc:IsFaceup() end
	if chk==0 then return e:GetHandler():IsCanRemoveCounter(tp,0x21,1,REASON_EFFECT)
		and Duel.IsExistingTarget(Card.IsFaceup,tp,LOCATION_ONFIELD,0,1,e:GetHandler(),0x21,1) end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(98162021,1))
	Duel.SelectTarget(tp,Card.IsFaceup,tp,LOCATION_ONFIELD,0,1,1,e:GetHandler(),0x21,1)
end
function c4694209.addc2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetCounter(0x21)==0 then return end
	c:RemoveCounter(tp,0x21,1,REASON_EFFECT)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		tc:AddCounter(0x21,1)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EFFECT_DESTROY_REPLACE)
		e1:SetTarget(c4694209.reptg)
		e1:SetOperation(c4694209.repop)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
	end
end
function c4694209.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetCounter(0x21)>0 end
	return true
end
function c4694209.repop(e,tp,eg,ep,ev,re,r,rp,chk)
	e:GetHandler():RemoveCounter(tp,0x21,1,REASON_EFFECT)
end
