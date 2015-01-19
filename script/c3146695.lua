--サイコ・リアクター
function c3146695.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c3146695.regcon)
	e1:SetOperation(c3146695.regop)
	c:RegisterEffect(e1)
end
function c3146695.cfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_PSYCHO)
end
function c3146695.regcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c3146695.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c3146695.regop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c3146695.cfilter,tp,LOCATION_MZONE,0,nil)
	local tc=g:GetFirst()
	while tc do
		tc:RegisterFlagEffect(3146695,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		tc=g:GetNext()
	end
	g:KeepAlive()
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetDescription(aux.Stringid(3146695,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLED)
	e1:SetTarget(c3146695.target)
	e1:SetOperation(c3146695.operation)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetLabelObject(g)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_PHASE_START+PHASE_END)
	e2:SetReset(RESET_PHASE+PHASE_END)
	e2:SetCountLimit(1)
	e2:SetLabelObject(g)
	e2:SetOperation(c3146695.reset)
	Duel.RegisterEffect(e2,tp)
end
function c3146695.filter(c,g)
	return c:GetFlagEffect(3146695)>0 and g:IsContains(c)
end
function c3146695.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	local g=Group.FromCards(a,d)
	if chk==0 then return d and g:IsExists(c3146695.filter,1,nil,e:GetLabelObject()) end
	local rg=g:Filter(Card.IsRelateToBattle,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,rg,rg:GetCount(),0,0)
end
function c3146695.operation(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	local g=Group.FromCards(a,d)
	local rg=g:Filter(Card.IsRelateToBattle,nil)
	Duel.Remove(rg,POS_FACEUP,REASON_EFFECT)
end
function c3146695.reset(e,tp,eg,ep,ev,re,r,rp)
	e:GetLabelObject():DeleteGroup()
end
