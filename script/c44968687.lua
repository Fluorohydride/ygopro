--伝説のフィッシャーマン三世
function c44968687.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c44968687.spcon)
	e2:SetOperation(c44968687.spop)
	c:RegisterEffect(e2)
	--remove
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_REMOVE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetTarget(c44968687.rmtg)
	e3:SetOperation(c44968687.rmop)
	c:RegisterEffect(e3)
	--indes
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e4:SetValue(1)
	c:RegisterEffect(e4)
	local e5=e4:Clone()
	e5:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	c:RegisterEffect(e5)
	--immune
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCode(EFFECT_IMMUNE_EFFECT)
	e6:SetValue(c44968687.efilter)
	c:RegisterEffect(e6)
	--double damage
	local e7=Effect.CreateEffect(c)
	e7:SetType(EFFECT_TYPE_IGNITION)
	e7:SetRange(LOCATION_MZONE)
	e7:SetCountLimit(1)
	e7:SetTarget(c44968687.damtg)
	e7:SetOperation(c44968687.damop)
	c:RegisterEffect(e7)
end
function c44968687.spcon(e,c)
	if c==nil then return true end
	return Duel.CheckReleaseGroup(c:GetControler(),Card.IsCode,1,nil,3643300)
end
function c44968687.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local g=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,3643300)
	Duel.Release(g,REASON_COST)
end
function c44968687.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToRemove,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c44968687.rmop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,0,LOCATION_MZONE,nil)
	if Duel.Remove(g,POS_FACEUP,REASON_EFFECT)~=0 then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_ATTACK)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e:GetHandler():RegisterEffect(e1)
	end
end
function c44968687.efilter(e,te)
	return te:IsActiveType(TYPE_SPELL+TYPE_TRAP)
end
function c44968687.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_REMOVED)>0 end
	local g=Duel.GetFieldGroup(tp,0,LOCATION_REMOVED)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g,g:GetCount(),0,0)
end
function c44968687.damop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,0,LOCATION_REMOVED)
	if Duel.SendtoGrave(g,REASON_EFFECT+REASON_RETURN)~=0 then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetCode(EFFECT_CHANGE_DAMAGE)
		e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e1:SetTargetRange(0,1)
		e1:SetValue(c44968687.damval)
		e1:SetReset(RESET_PHASE+PHASE_END)
		Duel.RegisterEffect(e1,tp)
		Duel.RegisterFlagEffect(tp,44968687,RESET_PHASE+PHASE_END,0,1)
	end
end
function c44968687.damval(e,re,val,r,rp,rc)
	local tp=e:GetHandlerPlayer()
	if Duel.GetFlagEffect(tp,44968687)==0 or bit.band(r,REASON_BATTLE+REASON_EFFECT)==0 then return val end
	Duel.ResetFlagEffect(tp,44968687)
	return val*2
end
