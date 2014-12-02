--ボーン·テンプル·ブロック
function c47778083.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c47778083.target)
	e1:SetOperation(c47778083.operation)
	c:RegisterEffect(e1)
end
function c47778083.filter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c47778083.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then
		return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)~=0
			and Duel.IsExistingTarget(c47778083.filter,tp,0,LOCATION_GRAVE,1,nil,e,tp)
			and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
			and Duel.IsExistingTarget(c47778083.filter,1-tp,0,LOCATION_GRAVE,1,nil,e,1-tp)
			and Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local sg=Duel.SelectTarget(tp,c47778083.filter,tp,0,LOCATION_GRAVE,1,1,nil,e,tp)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
	local og=Duel.SelectTarget(1-tp,c47778083.filter,1-tp,0,LOCATION_GRAVE,1,1,nil,e,1-tp)
	local sc=sg:GetFirst()
	local oc=og:GetFirst()
	local g=Group.FromCards(sc,oc)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,2,PLAYER_ALL,0)
	e:SetLabelObject(sg)
end
function c47778083.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_EFFECT+REASON_DISCARD)==0 then return end
	local c=e:GetHandler()
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local fid=c:GetFieldID()
	local tc=g:GetFirst()
	while tc do
		if tc:IsRelateToEffect(e) and Duel.SpecialSummonStep(tc,0,1-tc:GetControler(),1-tc:GetControler(),false,false,POS_FACEUP) then
			tc:RegisterFlagEffect(47778083,RESET_EVENT+0x1fe0000,0,0,fid)
		end
		tc=g:GetNext()
	end
	Duel.SpecialSummonComplete()
	local e1=Effect.CreateEffect(c)
	e1:SetLabel(fid)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetCountLimit(1)
	e2:SetLabel(Duel.GetTurnCount())
	e2:SetLabelObject(e1)
	e2:SetCondition(c47778083.descon)
	e2:SetOperation(c47778083.desop)
	e2:SetReset(RESET_PHASE+PHASE_END,2)
	Duel.RegisterEffect(e2,tp)
end
function c47778083.desfilter(c,fid)
	return c:GetFlagEffectLabel(47778083)==fid
end
function c47778083.descon(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.IsExistingMatchingCard(c47778083.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,e:GetLabelObject():GetLabel()) then
		e:Reset()
		return false
	else return Duel.GetTurnCount()~=e:GetLabel() end
end
function c47778083.desop(e,tp,eg,ep,ev,re,r,rp)
	local tg=Duel.GetMatchingGroup(c47778083.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,e:GetLabelObject():GetLabel())
	Duel.Destroy(tg,REASON_EFFECT)
end
