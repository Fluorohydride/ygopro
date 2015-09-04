--オーバー・デッド・ライン
function c87046457.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c87046457.acttg)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(87046457,0))
	e2:SetCategory(CATEGORY_ATKCHANGE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTarget(c87046457.target)
	e2:SetOperation(c87046457.operation)
	c:RegisterEffect(e2)
	--atk
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_UPDATE_ATTACK)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e3:SetCondition(c87046457.atkcon)
	e3:SetTarget(c87046457.atktg)
	e3:SetValue(1000)
	c:RegisterEffect(e3)
	local g=Group.CreateGroup()
	g:KeepAlive()
	e2:SetLabelObject(g)
	e3:SetLabelObject(g)
end
function c87046457.acttg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_SZONE)
	e1:SetCondition(c87046457.descon)
	e1:SetOperation(c87046457.desop)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
	c:RegisterEffect(e1)
	c:SetTurnCounter(0)
end
function c87046457.descon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c87046457.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ct=c:GetTurnCounter()
	ct=ct+1
	c:SetTurnCounter(ct)
	if ct==2 then
		Duel.Destroy(c,REASON_EFFECT)
	end
end
function c87046457.filter(c,e,tp)
	return c:IsFaceup() and c:IsRace(RACE_PLANT) and c:IsControler(tp) and c:IsPreviousLocation(LOCATION_GRAVE) and (not e or c:IsRelateToEffect(e))
end
function c87046457.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c87046457.filter,1,nil,nil,tp) end
	Duel.SetTargetCard(eg)
end
function c87046457.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local atkg=e:GetLabelObject()
	if c:GetFlagEffect(87046457)==0 then
		c:RegisterFlagEffect(87046457,RESET_EVENT+0x1ff0000,0,1)
		atkg:Clear()
	end
	local g=eg:Filter(c87046457.filter,nil,e,tp)
	local tc=g:GetFirst()
	while tc do
		tc:RegisterFlagEffect(87046457,RESET_EVENT+0x1fe0000,0,1)
		atkg:AddCard(tc)
		tc=g:GetNext()
	end
end
function c87046457.atkcon(e)
	return e:GetHandler():GetFlagEffect(87046457)~=0
end
function c87046457.atktg(e,c)
	return e:GetLabelObject():IsContains(c) and c:GetFlagEffect(87046457)~=0
end
