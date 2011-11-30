--X－セイバー ソウザ
function c63612442.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(Card.IsSetCard,0x100d),1)
	c:EnableReviveLimit()
	--get effect
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(63612442,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c63612442.cost)
	e1:SetTarget(c63612442.target)
	e1:SetOperation(c63612442.operation)
	c:RegisterEffect(e1)
end
function c63612442.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsSetCard,1,e:GetHandler(),0x100d) end
	local g=Duel.SelectReleaseGroup(tp,Card.IsSetCard,1,1,e:GetHandler(),0x100d)
	Duel.Release(g,REASON_COST)
end
function c63612442.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:GetFlagEffect(63612442)==0 or c:GetFlagEffect(63612443)==0 end
	local t1=c:GetFlagEffect(63612442)
	local t2=c:GetFlagEffect(63612443)
	local op=0
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(63612442,0))
	if t1==0 and t2==0 then
		op=Duel.SelectOption(tp,aux.Stringid(63612442,1),aux.Stringid(63612442,2))
	elseif t1==0 then op=Duel.SelectOption(tp,aux.Stringid(63612442,1))
	else Duel.SelectOption(tp,aux.Stringid(63612442,2)) op=1 end
	e:SetLabel(op)
	if op==0 then c:RegisterFlagEffect(63612442,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	else c:RegisterFlagEffect(63612443,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1) end
end
function c63612442.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if e:GetLabel()==0 then
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(63612442,3))
		e1:SetCategory(CATEGORY_DESTROY)
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
		e1:SetCode(EVENT_BATTLE_START)
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
		e1:SetCondition(c63612442.descon)
		e1:SetTarget(c63612442.destg)
		e1:SetOperation(c63612442.desop)
		c:RegisterEffect(e1)
	else
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
		e1:SetRange(LOCATION_MZONE)
		e1:SetValue(c63612442.efilter)
		c:RegisterEffect(e1)
	end
end
function c63612442.descon(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	if d==e:GetHandler() then d=Duel.GetAttacker() end
	e:SetLabelObject(d)
	return d~=nil
end
function c63612442.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,e:GetLabelObject(),1,0,0)
end
function c63612442.desop(e,tp,eg,ep,ev,re,r,rp)
	local d=e:GetLabelObject()
	if d:IsRelateToBattle() then
		Duel.Destroy(d,REASON_EFFECT)
	end
end
function c63612442.efilter(e,re)
	return re:GetOwner():IsType(TYPE_TRAP)
end
