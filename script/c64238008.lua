--リニアキャノン
function c64238008.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c64238008.cost)
	e1:SetTarget(c64238008.target)
	e1:SetOperation(c64238008.activate)
	c:RegisterEffect(e1)
	Duel.AddCustomActivityCounter(64238008,ACTIVITY_CHAIN,c64238008.chainfilter)
end
function c64238008.chainfilter(re,tp,cid)
	return not (re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsActiveType(TYPE_SPELL))
end
function c64238008.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	if chk==0 then return Duel.GetCustomActivityCount(64238008,tp,ACTIVITY_CHAIN)==0 and Duel.CheckReleaseGroup(tp,nil,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,nil,1,1,nil)
	local atk=g:GetFirst():GetTextAttack()/2
	if atk<0 then atk=0 end
	e:SetLabel(atk)
	Duel.Release(g,REASON_COST)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EFFECT_CANNOT_ACTIVATE)
	e1:SetTargetRange(1,1)
	e1:SetValue(c64238008.aclimit)
	e1:SetReset(RESET_PHASE+PHASE_END,1)
	Duel.RegisterEffect(e1,tp)
end
function c64238008.aclimit(e,re,tp)
	return re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsActiveType(TYPE_SPELL)
end
function c64238008.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local res=e:GetLabel()~=0
		e:SetLabel(0)
		return res
	end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,e:GetLabel())
	e:SetLabel(0)
end
function c64238008.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
