--トゥーン・キャノン・ソルジャー
function c79875176.initial_effect(c)
	--cannot attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetOperation(c79875176.atklimit)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e3)
	--destroy
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCode(EVENT_LEAVE_FIELD)
	e4:SetCondition(c79875176.sdescon)
	e4:SetOperation(c79875176.sdesop)
	c:RegisterEffect(e4)
	--direct attack
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_DIRECT_ATTACK)
	e5:SetCondition(c79875176.dircon)
	c:RegisterEffect(e5)
	--damage
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(79875176,0))
	e6:SetCategory(CATEGORY_DAMAGE)
	e6:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e6:SetType(EFFECT_TYPE_IGNITION)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCost(c79875176.damcost)
	e6:SetTarget(c79875176.damtg)
	e6:SetOperation(c79875176.damop)
	c:RegisterEffect(e6)
end
function c79875176.atklimit(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_ATTACK)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e:GetHandler():RegisterEffect(e1)
end
function c79875176.sfilter(c)
	return c:IsReason(REASON_DESTROY) and c:IsCode(15259703) and c:IsPreviousLocation(LOCATION_ONFIELD)
end
function c79875176.sdescon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c79875176.sfilter,1,nil)
end
function c79875176.sdesop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT)
end
function c79875176.dirfilter1(c)
	return c:IsFaceup() and c:IsCode(15259703)
end
function c79875176.dirfilter2(c)
	return c:IsFaceup() and c:IsType(TYPE_TOON)
end
function c79875176.dircon(e)
	return Duel.IsExistingMatchingCard(c79875176.dirfilter1,e:GetHandlerPlayer(),LOCATION_ONFIELD,0,1,nil)
		and not Duel.IsExistingMatchingCard(c79875176.dirfilter2,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
function c79875176.damcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,nil,1,nil) end
	local sg=Duel.SelectReleaseGroup(tp,nil,1,1,nil)
	Duel.Release(sg,REASON_COST)
end
function c79875176.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(500)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,500)
end
function c79875176.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
