--剣闘獣オクタビウス
function c29590752.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(29590752,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c29590752.descon)
	e1:SetTarget(c29590752.destg)
	e1:SetOperation(c29590752.desop)
	c:RegisterEffect(e1)
	--discard or return
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(29590752,1))
	e2:SetCategory(CATEGORY_HANDES+CATEGORY_TODECK)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c29590752.dcon)
	e2:SetTarget(c29590752.dtg)
	e2:SetOperation(c29590752.dop)
	c:RegisterEffect(e2)
end
function c29590752.descon(e,tp,eg,ep,ev,re,r,rp)
	local st=e:GetHandler():GetSummonType()
	return st>=(SUMMON_TYPE_SPECIAL+100) and st<(SUMMON_TYPE_SPECIAL+150)
end
function c29590752.desfilter(c)
	return c:IsFacedown() and c:GetSequence()~=5 and c:IsDestructable()
end
function c29590752.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c29590752.desfilter(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c29590752.desfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c29590752.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsFacedown() and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c29590752.dcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetBattledGroupCount()>0 and Duel.GetTurnPlayer()==tp
end
function c29590752.dtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(29590753)==0 end
	e:GetHandler():RegisterFlagEffect(29590753,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_BATTLE,EFFECT_FLAG_OATH,1)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,1,tp,0)
end
function c29590752.dop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsFaceup() or not c:IsRelateToEffect(e) then return end
	if Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)>0 and Duel.SelectYesNo(tp,aux.Stringid(29590752,2)) then
		Duel.DiscardHand(tp,aux.TRUE,1,1,REASON_EFFECT+REASON_DISCARD)
	else
		Duel.SendtoDeck(c,nil,2,REASON_EFFECT)
	end
end
