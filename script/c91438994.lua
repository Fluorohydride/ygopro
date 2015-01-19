--A・ジェネクス・ベルフレイム
function c91438994.initial_effect(c)
	--add counter
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetOperation(c91438994.addc1)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_REMOVE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c91438994.addc2)
	c:RegisterEffect(e2)
	--attackup
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_UPDATE_ATTACK)
	e3:SetValue(c91438994.attackup)
	c:RegisterEffect(e3)
	--damage
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e4:SetCode(EVENT_LEAVE_FIELD_P)
	e4:SetOperation(c91438994.damp)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(91438994,0))
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e5:SetCategory(CATEGORY_DAMAGE)
	e5:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e5:SetCode(EVENT_BATTLE_DESTROYED)
	e5:SetCondition(c91438994.damcon)
	e5:SetTarget(c91438994.damtg)
	e5:SetOperation(c91438994.damop)
	e5:SetLabelObject(e4)
	c:RegisterEffect(e5)
end
function c91438994.filter1(c,tp)
	return c:GetPreviousControler()==tp and c:GetPreviousLocation()==LOCATION_MZONE
end
function c91438994.addc1(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c91438994.filter1,1,nil,tp) then
		e:GetHandler():AddCounter(0xa,1)
	end
end
function c91438994.filter2(c,tp)
	return c:GetPreviousControler()~=tp and c:GetPreviousLocation()==LOCATION_GRAVE
end
function c91438994.addc2(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c91438994.filter2,1,nil,tp) then
		e:GetHandler():AddCounter(0xa,2)
	end
end
function c91438994.attackup(e,c)
	return c:GetCounter(0xa)*100
end
function c91438994.damp(e,tp,eg,ep,ev,re,r,rp)
	e:SetLabel(e:GetHandler():GetCounter(0xa))
end
function c91438994.damcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE) and e:GetHandler():IsReason(REASON_BATTLE)
end
function c91438994.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	local ct=e:GetLabelObject():GetLabel()
	if chk==0 then return ct~=0 end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(ct*300)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,ct*300)
end
function c91438994.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
