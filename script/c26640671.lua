--蜘蛛の領域
function c26640671.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--target
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(26640671,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetTarget(c26640671.target)
	e2:SetOperation(c26640671.operation)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_BATTLED)
	e3:SetOperation(c26640671.regop)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e4:SetCountLimit(1)
	e4:SetOperation(c26640671.posop)
	c:RegisterEffect(e4)
	--poschange
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD)
	e5:SetRange(LOCATION_SZONE)
	e5:SetCode(EFFECT_CANNOT_CHANGE_POSITION)
	e5:SetTargetRange(0,LOCATION_MZONE)
	e5:SetTarget(c26640671.postg)
	c:RegisterEffect(e5)
end
function c26640671.filter(c,ec)
	return c:IsFaceup() and not ec:IsHasCardTarget(c) and c:IsRace(RACE_INSECT) 
end
function c26640671.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c26640671.filter(chkc,e:GetHandler()) end
	if chk==0 then return Duel.IsExistingTarget(c26640671.filter,tp,LOCATION_MZONE,0,1,nil,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,c26640671.filter,tp,LOCATION_MZONE,0,1,1,nil,e:GetHandler())
end
function c26640671.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if not c:IsRelateToEffect(e) or not tc:IsRelateToEffect(e) or tc:IsFacedown() then return end
	c:SetCardTarget(tc)
end
function c26640671.regop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if d and d:IsControler(1-tp) and e:GetHandler():IsHasCardTarget(a) then
		d:RegisterFlagEffect(26640671,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	elseif d and a:IsControler(1-tp) and e:GetHandler():IsHasCardTarget(d) then
		a:RegisterFlagEffect(26640671,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	end
end
function c26640671.pfilter(c)
	return c:GetFlagEffect(26640671)~=0 and c:IsAttackPos()
end
function c26640671.posop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c26640671.pfilter,tp,0,LOCATION_MZONE,nil)
	Duel.ChangePosition(g,POS_FACEUP_DEFENCE)
	local tc=g:GetFirst()
	while tc do
		tc:RegisterFlagEffect(26640672,RESET_EVENT+0x1fe0000,0,1)
		tc=g:GetNext()
	end
end
function c26640671.postg(e,c)
	return c:GetFlagEffect(26640672)~=0
end
