--ライトロード・プリースト ジェニス
function c83725008.initial_effect(c)
	--recover&damage
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetDescription(aux.Stringid(83725008,0))
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(c83725008.lpcon)
	e1:SetTarget(c83725008.lptg)
	e1:SetOperation(c83725008.lpop)
	c:RegisterEffect(e1)
	if c83725008.discard==nil then
		c83725008.discard=true
		c83725008[0]=false
		c83725008[1]=false
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
		e2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		e2:SetOperation(c83725008.reset)
		Duel.RegisterEffect(e2,0)
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
		e3:SetCode(EVENT_TO_GRAVE)
		e3:SetOperation(c83725008.set)
		Duel.RegisterEffect(e3,0)
	end
end
function c83725008.reset(e,tp,eg,ep,ev,re,r,rp)
	c83725008[0]=false
	c83725008[1]=false
end
function c83725008.cfilter(c)
	return c:GetPreviousLocation()==LOCATION_DECK
end
function c83725008.set(e,tp,eg,ep,ev,re,r,rp)
	if not re then return false end
	local rc=re:GetHandler()
	if bit.band(r,REASON_EFFECT)>0 and rc:IsSetCard(0x38) and eg:IsExists(c83725008.cfilter,1,nil) then
		c83725008[rp]=true
	end
end
function c83725008.lpcon(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer() and c83725008[tp]
end
function c83725008.lptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,500)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,500)
end
function c83725008.lpop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetControler()~=tp or not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	Duel.Recover(tp,500,REASON_EFFECT)
	Duel.Damage(1-tp,500,REASON_EFFECT)
end
